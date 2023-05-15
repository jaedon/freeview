/**
 * scart_null.c
*/

/**
 * @defgroup		DI_SCART_NULL : SCART_NULL module
 * @author		Chung Dong Chul
 * @note			
 * @brief			
 * @file 			scart_null.c
*/

#include "vkernel.h"
#include "drv_scart.h"
#include "scart_null.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static DRV_SCART_OPERATION* s_pScartOp;



void Scart_I2C_Write_For_Debug(unsigned char data, unsigned char subaddr)
{
	HAPPY(data);
	HAPPY(subaddr);
}

/**************************************
 *	Register Value Dump
 **************************************/
void Scart_REG_Dump(void)
{
}

/*******************/
/*	Set Normal Mode*/
/*******************/
void Scart_Normal_Mode(void)
{
}


/*********************************/
/*   Change VCR Output enable status  */
/*********************************/
void Scart_VCR_Output_Off(void)
{
}

void Scart_VCR_Output_On(void)
{
}

/*******************************/
/*	Change TV Output enable status */
/*******************************/
void Scart_TV_Output_Off(void)
{
}
void Scart_TV_Output_On(void)
{
}

/****************************************/
/*	Change RF Modulator Output enable status  */
/****************************************/

void Scart_RF_Output_Off(void)
{
}
void Scart_RF_Output_On(void)
{
}

/*********************************/
/* Change Cinch Output enable status  */
/*********************************/
void Scart_Cinch_Output_Off	(void)
{
}
void Scart_Cinch_Output_On(void)
{
}

/***************************/
/* Change Video Output Format */
/***************************/
// 4:3 ratio -> 16:9 ratio On
void Scart_Wide_Mode_On(void)
{
}

//16:9 ratio -> 4:3 ratio On
void Scart_Wide_Mode_Off(void)
{
}

void Scart_TVMono_Mode_On(void)
{
}

void Scart_TVMono_Mode_Off(void)
{
}

void Scart_VCRMono_Mode_On(void)
{
}

void Scart_VCRMono_Mode_Off(void)
{
}

void Scart_VCRbypass_Mode_On(void)
{
}

void Scart_VCRbypass_Mode_Off(void)
{
}

int Scart_Check_TV_Input(void)
{
	return 0; 
}

int Scart_Check_VCR_Input(void)
{
	return 0;
}

int Scart_Check_VCR_ScreenFormat(void)
{
	return 1 ;	/* 4 : 3 */
}

int Scart_Check_TV_ScreenFormat(void)
{
	return 1 ;	/* 4 : 3 */
}

void Scart_HW_Mute_On(void)
{
}

void Scart_HW_Mute_Off(void)
{
}

void Scart_TV_Audio_Mute_On(void)
{
}

void Scart_TV_Audio_Mute_Off(void)
{
}

void Scart_VCR_Mute_On(void)
{
}

void Scart_VCR_Mute_Off(void)
{
}

void Scart_TV_RGB_Mode_On(void)
{
}

void Scart_TV_CVBS_Mode_On(void)
{
}

/*********************************/
/* Change SVHS Output enable status  */
/*********************************/
void Scart_TV_SVHS_Mode_On(void)
{
}

void Scart_TV_SVHS_Mode_Off(void)
{
}

void Scart_VCR_CVBS_Mode_On(void)
{
}

void Scart_VCR_SVHS_Mode_On(void)
{
}

void Scart_HW_Bypass(unsigned char ucAutoDetection)
{
	HAPPY(ucAutoDetection);
}

void Scart_Sound_Level(unsigned char level)
{
	HAPPY(level);
}

void Scart_FunctionPin_On(void)
{
}

void Scart_FunctionPin_Off(void)
{
}

void Scart_Interrupt_Enable(void)
{
}

void Scart_Interrupt_Disable(void)
{
}

void Scart_Read(void)
{
}

void Scart_Init_ChipDriver(void)
{
	s_pScartOp=DRV_SCART_GetOpHandle();
	if (s_pScartOp!=NULL)
	{
		s_pScartOp->pfDump_Reg				= Scart_REG_Dump;
		s_pScartOp->pfWrite_Reg				= Scart_I2C_Write_For_Debug;
		s_pScartOp->pfSet_Normal_Mode		= Scart_Normal_Mode ;
		s_pScartOp->pfSet_VCR_Output_Off		= Scart_VCR_Output_Off ;
		s_pScartOp->pfReset_VCR_Output_Off	= Scart_VCR_Output_On ;
		s_pScartOp->pfSet_TV_Output_Off		= Scart_TV_Output_Off ;
		s_pScartOp->pfReset_TV_Output_Off		= Scart_TV_Output_On ;
		s_pScartOp->pfSet_RF_Output_Off		= Scart_RF_Output_Off ;
		s_pScartOp->pfReset_RF_Output_Off		= Scart_RF_Output_On ;
		s_pScartOp->pfSet_Cinch_Output_Off	= Scart_Cinch_Output_Off ;
		s_pScartOp->pfReset_Cinch_Output_Off	= Scart_Cinch_Output_On ;
		s_pScartOp->pfSet_TV_SVHS_Mode		= Scart_TV_SVHS_Mode_On;
		s_pScartOp->pfReset_TV_SVHS_Mode	= Scart_TV_SVHS_Mode_Off;
		s_pScartOp->pfSet_Wide_Mode			= Scart_Wide_Mode_On ;
		s_pScartOp->pfReset_Wide_Mode		= Scart_Wide_Mode_Off;
		s_pScartOp->pfSet_TVMono_Mode		= Scart_TVMono_Mode_On;
		s_pScartOp->pfReset_TVMono_Mode		= Scart_TVMono_Mode_Off ;
		s_pScartOp->pfSet_VCRMono_Mode		= Scart_VCRMono_Mode_On ;
		s_pScartOp->pfReset_VCRMono_Mode	= Scart_VCRMono_Mode_Off ;
		s_pScartOp->pfSet_VCRbypass_Mode	= Scart_VCRbypass_Mode_On ;
		s_pScartOp->pfReset_VCRbypass_Mode	= Scart_VCRbypass_Mode_Off ;
		s_pScartOp->pfCheck_TV_Input			= Scart_Check_TV_Input ;
		s_pScartOp->pfCheck_VCR_Input		= Scart_Check_VCR_Input ;
		s_pScartOp->pfCheck_VCR_ScreenFormat	= Scart_Check_VCR_ScreenFormat ;
		s_pScartOp->pfSet_TV_Mute			= Scart_TV_Audio_Mute_On ;
		s_pScartOp->pfReset_TV_Mute			= Scart_TV_Audio_Mute_Off ;
		s_pScartOp->pfSet_VCR_Mute			= Scart_VCR_Mute_On ;
		s_pScartOp->pfReset_VCR_Mute		= Scart_VCR_Mute_Off ;
		s_pScartOp->pfSet_FunctionPin_On		= Scart_FunctionPin_On ;
		s_pScartOp->pfReset_FunctionPin_On	= Scart_FunctionPin_Off ;
		//s_pScartOp->pfSet_RGB_Mode		= STV6412_Set_RGB_Mode ;
		//s_pScartOp->pfSet_CVBS_Mode		= STV6412_Set_CVBS_Mode ;
		s_pScartOp->pfSet_TV_RGB_Mode		= Scart_TV_RGB_Mode_On;
		s_pScartOp->pfSet_TV_CVBS_Mode		= Scart_TV_CVBS_Mode_On;
		s_pScartOp->pfSet_VCR_CVBS_Mode 	= Scart_VCR_CVBS_Mode_On;
		s_pScartOp->pfSet_VCR_SVHS_Mode 	= Scart_VCR_SVHS_Mode_On;
		s_pScartOp->pfSet_HW_Bypass			= Scart_HW_Bypass;
		s_pScartOp->pfSet_Sound_Level		= Scart_Sound_Level;
		s_pScartOp->pfSet_HW_Mute			= Scart_HW_Mute_On;
		s_pScartOp->pfReset_HW_Mute			= Scart_HW_Mute_Off;
		s_pScartOp->pfInterrupt_Enable        	= Scart_Interrupt_Enable;
		s_pScartOp->pfInterrupt_Disable       	= Scart_Interrupt_Disable;
		s_pScartOp->pfStatusRead 			= Scart_Read;
	}
} 

