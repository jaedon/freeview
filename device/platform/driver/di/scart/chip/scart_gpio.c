/*************************
*	Scart controlled with GPIO
*	@author: Sunwoo Lee
*	@data: 2010.07.07
*************************/

#include "drv_scart.h"
#include "scart_gpio.h"
#include "drv_gpio.h"

/*************************
*	NOTICE!!
*	We have two Scart with GPIO implementations.
*	The appropriate definition should be added in product.config file.
*	(CONFIG_SCART_GPIO_SLB1 / CONFIG_SCART_GPIO_SLB2)
*
*	Scart with GPIO older version
*	|-------------------------------------------------------------------|
*	|   Mode           |   Spec.      |   SLB1 Scart   |   SLB2 Scart   |
*	|-------------------------------------------------------------------|
*	|   Standby        |   0~2 V      |   H            |   Don't Care   |
*	|-------------------------------------------------------------------|
*	|   PowerOn 16:9   |   4.5~7 V    |   L            |   H            |
*	|-------------------------------------------------------------------|
*	|   PowerOn 4:3    |   9.5~12 V   |   L            |   L            |
*	|-------------------------------------------------------------------|
*
* 	Scart with GPIO new version
*	|-------------------------------------------------------------------|
*	|   Mode           |   Spec.      |   SLB1 Scart   |   SLB2 Scart   |
*	|-------------------------------------------------------------------|
*	|   Standby        |   0~2 V      |   Don't Care   |   L            |
*	|-------------------------------------------------------------------|
*	|   PowerOn 16:9   |   4.5~7 V    |   H            |   H            |
*	|-------------------------------------------------------------------|
*	|   PowerOn 4:3    |   9.5~12 V   |   L            |   H            |
*	|-------------------------------------------------------------------|
*************************/

static DRV_SCART_OPERATION* s_pScartOp;

void P_SCART_GPIO_I2C_Write_For_Debug(unsigned char data, unsigned char subaddr)
{
	data = data;
	subaddr = subaddr;
}

void P_SCART_GPIO_REG_Dump(void)
{
}

void P_SCART_GPIO_Normal_Mode(void)
{
}

void P_SCART_GPIO_VCR_Output_Off(void)
{
}

void P_SCART_GPIO_VCR_Output_On(void)
{
}

void P_SCART_GPIO_TV_Output_Off(void)
{
}

void P_SCART_GPIO_TV_Output_On(void)
{
}

void P_SCART_GPIO_RF_Output_Off(void)
{
}

void P_SCART_GPIO_RF_Output_On(void)
{
}

void P_SCART_GPIO_Cinch_Output_Off	(void)
{
}

void P_SCART_GPIO_Cinch_Output_On(void)
{
}

void P_SCART_GPIO_Wide_Mode_On(void)
{
#if defined(CONFIG_SCART_GPIO_SLB1)
	DRV_GPIO_Write(GPIO_ID_SLB2_SCART, GPIO_HI);
#elif defined(CONFIG_SCART_GPIO_SLB2)
	DRV_GPIO_Write(GPIO_ID_SLB1_SCART, GPIO_HI);
#endif
}

void P_SCART_GPIO_Wide_Mode_Off(void)
{
#if defined(CONFIG_SCART_GPIO_SLB1)
	DRV_GPIO_Write(GPIO_ID_SLB2_SCART, GPIO_LOW);
#elif defined(CONFIG_SCART_GPIO_SLB2)
	DRV_GPIO_Write(GPIO_ID_SLB1_SCART, GPIO_LOW);
#endif
}

void P_SCART_GPIO_TVMono_Mode_On(void)
{
}

void P_SCART_GPIO_TVMono_Mode_Off(void)
{
}

void P_SCART_GPIO_VCRMono_Mode_On(void)
{
}

void P_SCART_GPIO_VCRMono_Mode_Off(void)
{
}

void P_SCART_GPIO_VCRbypass_Mode_On(void)
{
}

void P_SCART_GPIO_VCRbypass_Mode_Off(void)
{
}

int P_SCART_GPIO_Check_TV_Input(void)
{
	return 0; 
}

int P_SCART_GPIO_Check_VCR_Input(void)
{
	return 0;
}

int P_SCART_GPIO_Check_VCR_ScreenFormat(void)
{
	return 1;
}

int P_SCART_GPIO_Check_TV_ScreenFormat(void)
{
	return 1;
}

void P_SCART_GPIO_HW_Mute_On(void)
{
}

void P_SCART_GPIO_HW_Mute_Off(void)
{
}

void P_SCART_GPIO_TV_Audio_Mute_On(void)
{
}

void P_SCART_GPIO_TV_Audio_Mute_Off(void)
{
}

void P_SCART_GPIO_VCR_Mute_On(void)
{
}

void P_SCART_GPIO_VCR_Mute_Off(void)
{
}

void P_SCART_GPIO_TV_RGB_Mode_On(void)
{
	DRV_GPIO_Write(GPIO_ID_FBO_SCART, GPIO_LOW);
}

void P_SCART_GPIO_TV_CVBS_Mode_On(void)
{
	DRV_GPIO_Write(GPIO_ID_FBO_SCART, GPIO_HI);
}

void P_SCART_GPIO_TV_SVHS_Mode_On(void)
{
}

void P_SCART_GPIO_TV_SVHS_Mode_Off(void)
{
}

void P_SCART_GPIO_VCR_CVBS_Mode_On(void)
{
}

void P_SCART_GPIO_VCR_SVHS_Mode_On(void)
{
}

void P_SCART_GPIO_HW_Bypass(unsigned char ucAutoDetection)
{
	ucAutoDetection = ucAutoDetection;
}

void P_SCART_GPIO_Sound_Level(unsigned char level)
{
	level = level;
}

void P_SCART_GPIO_FunctionPin_On(void)
{
#if defined(CONFIG_SCART_GPIO_SLB1)
	DRV_GPIO_Write(GPIO_ID_SLB1_SCART, GPIO_LOW);
#elif defined(CONFIG_SCART_GPIO_SLB2)
	DRV_GPIO_Write(GPIO_ID_SLB2_SCART, GPIO_HI);
#endif
}

void P_SCART_GPIO_FunctionPin_Off(void)
{
#if defined(CONFIG_SCART_GPIO_SLB1)
	DRV_GPIO_Write(GPIO_ID_SLB1_SCART, GPIO_HI);
#elif defined(CONFIG_SCART_GPIO_SLB2)
	DRV_GPIO_Write(GPIO_ID_SLB2_SCART, GPIO_LOW);
#endif
}

void P_SCART_GPIO_Interrupt_Enable(void)
{
}

void P_SCART_GPIO_Interrupt_Disable(void)
{
}

void P_SCART_GPIO_Read(void)
{
}

void Scart_Init_ChipDriver(void)
{
	s_pScartOp=DRV_SCART_GetOpHandle();
	if (s_pScartOp!=NULL)
	{
		s_pScartOp->pfDump_Reg				= P_SCART_GPIO_REG_Dump;
		s_pScartOp->pfWrite_Reg				= P_SCART_GPIO_I2C_Write_For_Debug;
		s_pScartOp->pfSet_Normal_Mode		= P_SCART_GPIO_Normal_Mode ;
		s_pScartOp->pfSet_VCR_Output_Off	= P_SCART_GPIO_VCR_Output_Off ;
		s_pScartOp->pfReset_VCR_Output_Off	= P_SCART_GPIO_VCR_Output_On ;
		s_pScartOp->pfSet_TV_Output_Off		= P_SCART_GPIO_TV_Output_Off ;
		s_pScartOp->pfReset_TV_Output_Off	= P_SCART_GPIO_TV_Output_On ;
		s_pScartOp->pfSet_RF_Output_Off		= P_SCART_GPIO_RF_Output_Off ;
		s_pScartOp->pfReset_RF_Output_Off	= P_SCART_GPIO_RF_Output_On ;
		s_pScartOp->pfSet_Cinch_Output_Off	= P_SCART_GPIO_Cinch_Output_Off ;
		s_pScartOp->pfReset_Cinch_Output_Off	= P_SCART_GPIO_Cinch_Output_On ;
		s_pScartOp->pfSet_TV_SVHS_Mode		= P_SCART_GPIO_TV_SVHS_Mode_On;
		s_pScartOp->pfReset_TV_SVHS_Mode	= P_SCART_GPIO_TV_SVHS_Mode_Off;
		s_pScartOp->pfSet_Wide_Mode			= P_SCART_GPIO_Wide_Mode_On ;
		s_pScartOp->pfReset_Wide_Mode		= P_SCART_GPIO_Wide_Mode_Off;
		s_pScartOp->pfSet_TVMono_Mode		= P_SCART_GPIO_TVMono_Mode_On;
		s_pScartOp->pfReset_TVMono_Mode		= P_SCART_GPIO_TVMono_Mode_Off ;
		s_pScartOp->pfSet_VCRMono_Mode		= P_SCART_GPIO_VCRMono_Mode_On ;
		s_pScartOp->pfReset_VCRMono_Mode	= P_SCART_GPIO_VCRMono_Mode_Off ;
		s_pScartOp->pfSet_VCRbypass_Mode	= P_SCART_GPIO_VCRbypass_Mode_On ;
		s_pScartOp->pfReset_VCRbypass_Mode	= P_SCART_GPIO_VCRbypass_Mode_Off ;
		s_pScartOp->pfCheck_TV_Input		= P_SCART_GPIO_Check_TV_Input ;
		s_pScartOp->pfCheck_VCR_Input		= P_SCART_GPIO_Check_VCR_Input ;
		s_pScartOp->pfCheck_VCR_ScreenFormat	= P_SCART_GPIO_Check_VCR_ScreenFormat ;
		s_pScartOp->pfSet_TV_Mute			= P_SCART_GPIO_TV_Audio_Mute_On ;
		s_pScartOp->pfReset_TV_Mute			= P_SCART_GPIO_TV_Audio_Mute_Off ;
		s_pScartOp->pfSet_VCR_Mute			= P_SCART_GPIO_VCR_Mute_On ;
		s_pScartOp->pfReset_VCR_Mute		= P_SCART_GPIO_VCR_Mute_Off ;
		s_pScartOp->pfSet_FunctionPin_On	= P_SCART_GPIO_FunctionPin_On ;
		s_pScartOp->pfReset_FunctionPin_On	= P_SCART_GPIO_FunctionPin_Off ;
		s_pScartOp->pfSet_TV_RGB_Mode		= P_SCART_GPIO_TV_RGB_Mode_On;
		s_pScartOp->pfSet_TV_CVBS_Mode		= P_SCART_GPIO_TV_CVBS_Mode_On;
		s_pScartOp->pfSet_VCR_CVBS_Mode 	= P_SCART_GPIO_VCR_CVBS_Mode_On;
		s_pScartOp->pfSet_VCR_SVHS_Mode 	= P_SCART_GPIO_VCR_SVHS_Mode_On;
		s_pScartOp->pfSet_HW_Bypass			= P_SCART_GPIO_HW_Bypass;
		s_pScartOp->pfSet_Sound_Level		= P_SCART_GPIO_Sound_Level;
		s_pScartOp->pfSet_HW_Mute			= P_SCART_GPIO_HW_Mute_On;
		s_pScartOp->pfReset_HW_Mute			= P_SCART_GPIO_HW_Mute_Off;
		s_pScartOp->pfInterrupt_Enable      = P_SCART_GPIO_Interrupt_Enable;
		s_pScartOp->pfInterrupt_Disable     = P_SCART_GPIO_Interrupt_Disable;
		s_pScartOp->pfStatusRead 			= P_SCART_GPIO_Read;
	}
} 

